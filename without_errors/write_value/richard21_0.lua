require("ifj21")

function swap(a, b)
   return b, a
end

function main()
   local a = 8
   local b = 6
   a, b = swap(a, b)
end

main()
