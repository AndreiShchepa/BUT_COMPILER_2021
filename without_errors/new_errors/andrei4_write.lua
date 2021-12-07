require("ifj21")

function foo()
   local b = 7 + 10
   local a = ord("STR", b)
   write(a, "\n")
   return nil
end

function main()
   write("bbb \n"); local a = foo()
   write(a)
   write("bbb\n")
   return
end

main()
