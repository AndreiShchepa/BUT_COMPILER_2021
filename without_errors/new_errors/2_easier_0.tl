require "ifj21"

function main()
	local x: integer = 4

	while 1 do
		x=x*2
		-- Creative way to check if abs(root - x) < eps
		if (x>100) then
			write(x)
			return
		else
		end
	end
end

main()