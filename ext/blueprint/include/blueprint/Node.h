#pragma once

#include <SM_Vector.h>
#include <painting2/Color.h>

#include <vector>
#include <memory>

namespace bp
{

class Node
{
public:
	
	void SetStyle(const sm::vec2& size, const pt2::Color& color);

public:
	struct Style
	{
		sm::vec2   size;
		pt2::Color color;
	};

	struct Port
	{
		std::string           name;
		std::shared_ptr<Node> node = nullptr;
	};

	const Style& GetStyle() const { return m_style; }

private:
	Style m_style;

	std::vector<Port> m_input;
	std::vector<Port> m_output;

}; // Node

}