library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity decoder is
    port (
        clk: in std_logic;
        instr: out std_logic_vector(7 downto 0)
    );
end entity decoder;

architecture rtl of decoder is
    
begin
    instr <= "11110000";
end architecture rtl;