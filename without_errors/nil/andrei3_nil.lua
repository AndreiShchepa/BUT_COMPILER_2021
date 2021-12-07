require("ifj21")

function foo()
   local a = substr("STR", nil, 2)
   write(a, "\n")
   return nil
end

function main()
   write("bbb\n")
   local a = foo()
   write(a)
   write("bbb\n")
   return
end

main()
