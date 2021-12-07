require("ifj21")

function sqrt(n, eps)

   local x = n

   if n == nil then
      return nil
   else
   end

   if n < 0 then
      return nil
   else
   end

   while 1 do
      write("jsem to")

      local root = 0.5 * (x + n / x)


      if ((root - x) < eps) then
         if ((0 - eps) < (root - x)) then
            return x
         else
         end
      else
      end


      x = root
   end
end

function main()
   write("Type your number: ")

   local n = readn()
   local eps = 0.000001
   write(n, "\n")

   local result = sqrt(n, eps)

   if result then
      write(result, "\n")
   else
      write("Could not calculate\n")
   end
end

main()
