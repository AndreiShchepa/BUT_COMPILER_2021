require("ifj21")

function foo(x, y, z, t)
   local a = x
   local b = y
   local c = z + t
   return a, b, c
end

function main()
   local q1
   local q2
   local q3

   local p1 = 1
   local p2 = 2
   local p3 = 3
   local p4 = 4

   q1, q2, q3 = foo(p1, p2, p3, p4)
   write(q1, "\n", q2, "\n", q3, "\n")
end

main()
