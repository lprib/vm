library ieee;
use ieee.std_logic_1164.all;

entity toplevel is
    port (
        leds : out std_logic_vector(4 - 1 downto 0) := "0000";
        buttons: in std_logic_vector(4 - 1 downto 0)
    );
end entity toplevel;

architecture behav of toplevel is
    component negate is
        port (
            a: in std_logic;
            y: out std_logic
        );
    end component negate;
begin
    negator: negate port map (a => buttons(0), y => leds(0));
    leds(1) <= buttons(1);
end architecture behav;