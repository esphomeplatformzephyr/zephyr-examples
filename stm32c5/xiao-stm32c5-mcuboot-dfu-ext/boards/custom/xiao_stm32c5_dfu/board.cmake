# STM32 ROM DFU bootloader (BOOT button + reset): USB ID 0483:df11, flash at
# alt 0, DfuSe protocol.
board_runner_args(dfu-util "--pid=0483:df11" "--alt=0" "--dfuse")
include(${ZEPHYR_BASE}/boards/common/dfu-util.board.cmake)
# The board's own board.cmake already chose stm32cubeprogrammer; replace it.
board_set_flasher(dfu-util)
