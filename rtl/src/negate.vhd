library ieee;
use ieee.std_logic_1164.all;

entity negate is
    port (
        a: in std_logic;
        y: out std_logic
    );
end entity negate;

architecture behav of negate is
begin
    y <= not a;
end architecture behav;