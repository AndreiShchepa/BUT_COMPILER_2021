require "ifj21"

function main(a:integer, b:float) -- uzivatelska funkce bez parametru
	--[[ scope 1
	if ()
		-- scope 2
		if ()
			-- scope 3 - deep = 3
			a:number = 0;
		end
	end
	]]
	write("a")
end

main(2, 2.0)  		-- prikaz hlavniho tela programu
