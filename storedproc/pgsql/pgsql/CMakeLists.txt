if (LIBPQ_FOUND)
    execute_process (COMMAND pg_config --sharedir OUTPUT_VARIABLE
                     PG_SHAREDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    install (FILES delivery.sql new_order.sql order_status.sql payment.sql
                   stock_level.sql
             DESTINATION "${PG_SHAREDIR}")
endif (LIBPQ_FOUND)
