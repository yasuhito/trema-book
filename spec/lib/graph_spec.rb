require 'graph'

describe Graph do
  Given(:data) do
    {
      s: [:a],
      a: [:s, :b, :c],
      b: [:a, :c, :d],
      c: [:a, :b, :d, :g],
      d: [:a, :b, :c],
      g: [:c]
    }
  end
  When(:route) { Graph.new(data).route(:s, :g) }
  Then { route == [:s, :a, :c, :g] }
end
