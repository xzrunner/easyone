-- create node RadiansToDegrees

require "moon.sg"

local node = moon.sg.new_node("sg_radians_to_degrees")
if node then
	node:set_pos(moon.mouse.get_position())
end