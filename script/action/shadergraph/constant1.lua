-- create node constant1

require "moon.sg"

local node = moon.sg.new_node("sg_constant1")
if node then
	node:set_pos(moon.mouse.get_position())
end