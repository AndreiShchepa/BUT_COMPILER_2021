require("ifj21")

function foo(x, y, z, t)
   local a = x
   local b = y
   local c = z + t
   return a, b
end

function foo2(x, y, z, t)
   local a = x
   local b = y
   local c = z + t
   return c, t
end

function main()
   local q1
   local q2
   local q3
   local q4

   local p1 = 1
   local p2 = 2
   local p3 = 3
   local p4 = 4

   q1, q2 = foo(p1, p2, p3, p4)
   write(q1, "\n", q2, "\n")

   q3, q4 = foo2(p1, p2, p3, p4)
   write(q3, "\n", q4, "\n")

end

main()
