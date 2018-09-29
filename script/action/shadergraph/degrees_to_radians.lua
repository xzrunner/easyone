-- create node DegreesToRadians

require "moon.sg"

local node = moon.sg.new_node("sg_degrees_to_radians")
if node then
	node:set_pos(moon.mouse.get_position())
end