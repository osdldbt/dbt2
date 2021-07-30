find_program (PG_CONFIG pg_config)
if (PG_CONFIG)
    execute_process (COMMAND ${PG_CONFIG} --sharedir OUTPUT_VARIABLE
                     PG_SHAREDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    install (FILES delivery.sql new_order.sql order_status.sql payment.sql
                   stock_level.sql
             DESTINATION "${PG_SHAREDIR}")
endif (PG_CONFIG)
