library ieee;
use ieee.std_logic_1164.all;

entity platform is
    port (
        clk : in std_logic;
        btn1, btn2, btn3, btn4 : inout std_logic;
        led1, led2, led3, led4 : out std_logic
    );
end entity platform;

architecture behav of platform is
    component toplevel is
        port (
            leds : out std_logic_vector(4 - 1 downto 0) := "0000";
            buttons: inout std_logic_vector(4 - 1 downto 0);
            clk: in std_logic
        );
    end component toplevel;
    
    signal neg: std_logic;
begin
    led4 <= not neg;

    top: toplevel port map (
        clk => clk,
        leds(0) => led1,
        leds(1) => led2,
        leds(2) => led3,
        leds(3) => neg,
        buttons(0) => btn1,
        buttons(1) => btn2,
        buttons(2) => btn3,
        buttons(3) => btn4
    );
end architecture behav;