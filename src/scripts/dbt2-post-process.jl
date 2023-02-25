#!/bin/sh
#
# This file is released under the terms of the Artistic License.
# Please see the file LICENSE, included in this package, for details.
#
# Copyright (C) 2006      Open Source Development Labs, Inc.
#               2014      2ndQuadrant, Ltd.
#               2006-2023 Mark Wong
#

#=
if [ $# -lt 1 ]; then
    echo "$(basename "$0") is the DBT-2 mix log analyzer"
    echo ""
    echo "Usage:"
    echo "  $(basename "$0") VERBOSE mix.log [mix-1.log [...]]"
    echo
fi

if [ ! "$VERBOSE" = "1" ]; then
	VERBOSE=0
fi

exec julia --color=no --startup-file=no "$0" $VERBOSE "$@"
=#

using CSV
using DataFrames
using Distributions
using Printf
using Statistics

const TransactionNames = Dict(
    "d" => "Delivery",
    "n" => "New Order",
    "o" => "Order Status",
    "p" => "Payment",
    "s" => "Stock Level"
)

function display_did_p(df, wdf, stats)
    min = minimum(wdf.wid)
    max = maximum(wdf.wid)

    pdf_wid_did = pivot_by_wid_did(df, stats)
    # FIXME: Should determine district ranged based on test parameters as
    #        opposed to hard coding the default10.
    for wid = min:max
        E = wdf[wdf.wid .== wid, :].nrow[1] / 10.0
        chi_square = 0
        for did = 1:10
            O = pdf_wid_did[(pdf_wid_did.wid .== wid) .&
                            (pdf_wid_did.did .== did), :].nrow[1]
            chi_square += (O - E)^2
        end
        chi_square /= E
        p = 1 - cdf(Chisq(9), chi_square)
        @printf("* Warehouse %d District chi-square p-value: %f\n", wid, p)
    end
end

function display_wid_p(wdf, stats)
    min = minimum(wdf.wid)
    max = maximum(wdf.wid)
    total = sum(wdf.nrow)
    E = total / (max - min + 1)
    chi_square = 0
    for wid = min:max
        O = wdf[wdf.wid .== wid, :].nrow[1]
        chi_square += (O - E)^2
    end
    chi_square /= E
    p = 1 - cdf(Chisq(max - min), chi_square)
    @printf("* Warehouse (%d-%d) chi-square p-value: %f\n", min, min, p)
end

function displayreport(summary_txn, summary_txn_code, stats)
    TotalTransactions = sum(summary_txn.nrow)

    @printf("============  =====  =========  =========  ===========  ")
    @printf("===========  =====\n")
    @printf("          ..     ..    Response Time (s)            ..")
    @printf("           ..     ..\n")
    @printf("------------  -----  --------------------  -----------  ")
    @printf("-----------  -----\n")
    @printf(" Transaction      %%   Average     90th %%        Total")
    @printf("    Rollbacks      %%\n")
    @printf("============  =====  =========  =========  ===========  ")
    @printf("===========  =====\n")

    rollbacks = 0
    for i in ["d", "n", "o", "p", "s"]
        row = summary_txn[(summary_txn.transaction .== i), :]
        try
            rollbacks = summary_txn_code[
                    (summary_txn_code.transaction .== i) .&
                    (summary_txn_code.code .== "R"), :].nrow[1]
        catch
            rollbacks = 0
        end

        @printf("%12s  %5.2f  %9.3f   %8.3f  %11d  %11d  %5.2f\n",
                TransactionNames[i],
                row.nrow[1] / TotalTransactions * 100,
                row.avg[1],
                row.q90[1],
                row.nrow[1],
                rollbacks,
                rollbacks / (row.nrow[1]) * 100)
    end
    @printf("============  =====  =========  =========  ===========  ")
    @printf("===========  =====\n")
    @printf("\n")
    @printf("* Throughput: %0.2f new-order transactions per minute (NOTPM)\n",
            summary_txn[(summary_txn.transaction .== "n"), :].nrow[1] /
                    (stats["SteadyStateTime"] / 60))
    @printf("* Duration: %0.1f minute(s)\n", stats["SteadyStateTime"] / 60)
    @printf("* Unknown Errors: %d\n", stats["errors"])
    @printf("* Ramp Up Time: %0.1f minute(s)\n", stats["RampupTime"] / 60)
end

function load(filenames)
    colnames = [
            "ctime",
            "transaction",
            "code",
            "response_time",
            "id",
            "wid",
            "did",
    ]
    df = mapreduce(vcat, filenames) do filename
        DataFrame(CSV.File(filename, header=colnames))
    end
    return df
end

function pivot_by_txn(df, stats)
    gdf = groupby(filter(row -> row["ctime"] > stats["StartTime"] &&
                         row["ctime"] < stats["TerminatedTime"] &&
                         row["code"] != "E", df),
                  [:transaction]; sort=true, skipmissing=true)
    summary = combine(gdf,
                 nrow,
                 :response_time => mean => :avg,
                 :response_time => (x -> quantile(x, .90)) => :q90
    )
    return summary
end

function pivot_by_txn_code(df, stats)
    gdf = groupby(filter(row -> row["ctime"] > stats["StartTime"] &&
                         row["ctime"] < stats["TerminatedTime"], df),
                  [:transaction, :code]; sort=true, skipmissing=true)
    summary = combine(gdf, nrow)
    return summary
end

function pivot_by_wid(df, stats)
    gdf = groupby(filter(row -> row["ctime"] > stats["StartTime"] &&
                         row["ctime"] < stats["TerminatedTime"], df),
                  [:wid]; skipmissing=true)
    summary = combine(gdf, nrow)
    return summary
end

function pivot_by_wid_did(df, stats)
    gdf = groupby(filter(row -> row["ctime"] > stats["StartTime"] &&
                         row["ctime"] < stats["TerminatedTime"], df),
                  [:wid, :did]; skipmissing=true)
    summary = combine(gdf, nrow)
    return summary
end

function prep(df)
    stats = Dict()

    stats["errors"] = count(y -> (y .== "E"), skipmissing(df.code))

    stats["StartTime"] = maximum(df[df.transaction .== "START", :ctime])
    stats["TerminatedTime"] = minimum(df[df.transaction .== "TERMINATED",
                                      :ctime])
    stats["SteadyStateTime"] = stats["TerminatedTime"] - stats["StartTime"]

    stats["ctime0"] = df[[1], [1]].ctime[1]
    stats["RampupTime"] = stats["StartTime"] - stats["ctime0"]

    return stats
end

function main()
    df = load(ARGS[2:end])
    stats = prep(df)
    pdf_txn = pivot_by_txn(df, stats)
    pdf_txn_code = pivot_by_txn_code(df, stats)
    displayreport(pdf_txn, pdf_txn_code, stats)
    if ARGS[1] == "1"
        wdf = pivot_by_wid(df, stats)
        display_wid_p(wdf, stats)
        display_did_p(df, wdf, stats)
    end
end

main()
