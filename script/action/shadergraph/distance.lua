-- create node Distance

require "moon.sg"

local node = moon.sg.new_node("sg_distance")
if node then
	node:set_pos(moon.mouse.get_position())
end