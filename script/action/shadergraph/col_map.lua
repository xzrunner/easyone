-- create node ColorMap

require "moon.sg"

local node = moon.sg.new_node("sg_col_map")
if node then
	node:set_pos(moon.mouse.get_position())
end