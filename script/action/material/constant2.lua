-- create node constant2

require "moon.mat"

local node = moon.mat.new_node("mat_constant2")
if node then
	node:set_pos(moon.mouse.get_position())
end