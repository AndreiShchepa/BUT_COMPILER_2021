require "ifj21"

function foo() : integer
	local b : integer = 7 + 10 
	local a : number = ord("", 1) 
	write(a, b, "\n")
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

