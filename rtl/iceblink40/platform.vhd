library ieee;
use ieee.std_logic_1164.all;

entity platform is
    port (
        clk : in std_logic;
        btn1, btn2, btn3, btn4 : in std_logic;
        led1, led2, led3, led4 : out std_logic
    );
end entity platform;

architecture behav of platform is
    component toplevel is
        port (
            leds : out std_logic_vector(4 - 1 downto 0);
            buttons: in std_logic_vector(4 - 1 downto 0)
        );
    end component toplevel;
begin
    top: toplevel port map (
        leds(0) => led1,
        leds(1) => led2,
        leds(2) => led3,
        leds(3) => led4,
        buttons(0) => btn1,
        buttons(1) => btn2,
        buttons(2) => btn3,
        buttons(3) => btn4
    );
end architecture behav;