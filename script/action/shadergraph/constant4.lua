-- create node constant4

require "moon.sg"

local node = moon.sg.new_node("sg_constant4")
if node then
	node:set_pos(moon.mouse.get_position())
end