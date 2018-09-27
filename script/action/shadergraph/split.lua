-- create node Split

require "moon.sg"

local node = moon.sg.new_node("sg_split")
if node then
	node:set_pos(moon.mouse.get_position())
end