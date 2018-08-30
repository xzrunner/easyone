-- create node flow_ctrl_branch

require "moon.bp"

local node = moon.bp.new_node("bp_flow_ctrl_branch")
if node then
	node:set_pos(moon.mouse.get_position())
end