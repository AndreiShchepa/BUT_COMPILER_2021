require "ifj21"

function foo() : integer
	local a : string = substr("STR", nil, 2) 
	write(a, "\n")
	return nil 
end

function main()
	write("bbb\n") 
	local a : integer = foo() 
	write(a)
	write("bbb\n")
	return
end

main()

