.text

.global io_hlt, io_sti, io_cli

io_hlt: //void io_hlt(void)
    hlt
    ret

io_sti:
    sti
    ret

io_cli:
    cli
    ret
