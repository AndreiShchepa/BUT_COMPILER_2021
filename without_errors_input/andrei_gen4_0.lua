require("ifj21")





function func4(limit)
   local num_f = readn()
   local num_i

   if num_i ~= nil then
      return
   else
      if num_f > limit then
         return
      else end

      num_i = tointeger(num_f)
   end

   num_i = func1(num_i, limit)

   write("\n", "Hop Hej, ", num_i, " numbers were written to output", "\n")
   return
end

function func1(num_f, limit)
   local break_cyklus = ""
   local counter = 0

   while break_cyklus do
      if num_f >= limit then
         break_cyklus = nil
      else
         write(num_f, "\t")
         num_f = num_f * num_f
         counter = counter + 1
      end
   end

   return counter
end

func4(1000)
