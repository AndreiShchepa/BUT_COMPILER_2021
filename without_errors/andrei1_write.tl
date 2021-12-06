require "ifj21"

function foo() : integer end
end 

function main()
	local a : integer = foo()

	if a -= nil then
		write("NEFUNGUJE NAM TOTO\n\010\x0A\xoa"), 
	else
		write("POHODA\n\010\x0A\xOa"),
	end
end

main()

