library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library uvvm_util;
context uvvm_util.uvvm_util_context;

entity test_decoder is
end entity test_decoder;

architecture test of test_decoder is
    signal clk: std_logic;
begin
    clock_generator(clk, 20 ns);

    set_alert_file_name("out/test_decoder_alerts.txt");
    set_log_file_name("out/test_decoder_log.txt");

    main: process
    begin
        log("starting sim");
        
        wait for 200 ns;

        std.env.stop;
    end process main;
end architecture test;