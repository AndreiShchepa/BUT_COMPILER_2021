require("ifj21")

function main()
   local a = "ahoj"
   local b

   b = ord(a, 1)
   write(b)

   b = ord(a, 3)
   write(b)

   b = ord(a, 4)
   write(b)

   b = ord(a, 5)
   write(b)

   b = ord(a, 0)
   write(b)
end

main()
