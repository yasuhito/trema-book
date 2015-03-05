require 'graph'

describe Graph do
  describe '#route' do
    context 'with valid data' do
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

    context 'with network topology data' do
      Given(:data) do
        {
          '192.168.0.1' => ['1:1'],
          1 => ['1:1', '1:2'],
          '1:1' => ['192.168.0.1', 1],
          '1:2' => [1, '2:2'],
          2 => ['2:1', '2:2'],
          '2:1' => [2, '192.168.0.2'],
          '2:2' => ['1:2', 2],
          '192.168.0.2' => ['2:1']
        }
      end
      When(:route) { Graph.new(data).route('192.168.0.1', '192.168.0.2') }
      Then do
        route ==
          ['192.168.0.1', '1:1', 1, '1:2', '2:2', 2, '2:1', '192.168.0.2']
      end
    end
  end
end
