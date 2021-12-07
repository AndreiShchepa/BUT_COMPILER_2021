require "ifj21"

function main():integer, integer
	local x: integer = 4

	while 1 do
		x=x*2
		if x > 8 then
			write(x)
			return x
		else
		end
	end
end

main()