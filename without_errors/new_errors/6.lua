require("ifj21")

function foo()
end


function main()
   local a = "314"
   local b = 314
   local c = 3.14
   write(a, "\n", b, "\n", c, "\n")
   a, b, c = foo()
   write(a, "\n", b, "\n", c, "\n")
end

main()
