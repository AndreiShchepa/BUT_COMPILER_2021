require "ifj21"

function main()
	local str: string= "ahoj"
	local str_ret: string

	str_ret = substr(str, 1, 2)
	write(str_ret, "\n", "\n")

	str_ret = substr(str, -1, 2)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, 1, -2)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, 0, 3)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, 0, 2)
	write(str_ret, "\n", "\n")
	
	str_ret = substr(str, 1, 3)
	write(str_ret, "\n", "\n")
	
end

main()
