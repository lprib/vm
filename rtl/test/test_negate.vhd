library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity test_negate is end;

architecture test of test_negate is
    component negate is
        port (
            a: in std_logic;
            y: out std_logic
        );
    end component negate;
    signal input: std_logic;
    signal output: std_logic;
begin
    uut: negate port map (a => input, y => output);

    do_test: process
    begin
        input <= '1';
        wait for 10 ms;
        input <= '0';
        wait for 10 ms;
        input <= '1';
        wait for 10 ms;
        assert false severity failure;
    end process do_test;
end architecture test;