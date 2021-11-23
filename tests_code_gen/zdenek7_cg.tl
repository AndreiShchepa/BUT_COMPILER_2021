require "ifj21"

function foo(b:integer) : integer
	return b, b+1
end

function main(a:integer)
	local c:integer
	local d:integer
	c, d = foo(a)
	write(c, d)
end

main(2)
