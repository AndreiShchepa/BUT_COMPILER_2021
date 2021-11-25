require "ifj21"

function foo(a:integer, b:integer):integer, integer, number
	return a, b
end

function main(a:integer, b:integer)
	local c:integer
	local d:integer
	c,d=foo(a,b)
	write(c,"\n", d, "\n")
end

main(1,2)
