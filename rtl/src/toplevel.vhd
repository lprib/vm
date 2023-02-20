library ieee;
use ieee.std_logic_1164.all;

entity toplevel is
    port (
        clk: in std_logic;
        instr: out std_logic_vector(7 downto 0)
    );
end entity toplevel;

architecture behav of toplevel is
begin
    dec : entity work.decoder(rtl) port map (instr => instr, clk => clk);
end architecture behav;