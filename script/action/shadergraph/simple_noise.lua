-- create node SimpleNoise

require "moon.sg"

local node = moon.sg.new_node("sg_simple_noise")
if node then
	node:set_pos(moon.mouse.get_position())
end