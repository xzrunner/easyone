-- create node RandomRange

require "moon.sg"

local node = moon.sg.new_node("sg_random_range")
if node then
	node:set_pos(moon.mouse.get_position())
end