require 'dijkstra'
require 'pio'

describe Dijkstra, '.new' do
  Given(:dijkstra) { Dijkstra.new(graph) }

  context 'with a valid graph' do
    Given(:graph) do
      {
        s: [:a],
        a: [:s, :b, :c],
        b: [:a, :c, :d],
        c: [:a, :b, :d, :g],
        d: [:a, :b, :c],
        g: [:c]
      }
    end

    describe '#run' do
      context 'with :s, :g' do
        When(:route) { dijkstra.run(:s, :g) }
        Then { route == [:s, :a, :c, :g] }
      end

      context 'with :s, :e' do
        When(:result) { dijkstra.run(:s, :e) }
        Then { result == Failure(RuntimeError, 'Node :e not found') }
      end
    end
  end

  context 'with an unreachable graph' do
    Given(:graph) do
      {
        s: [:a],
        a: [:s],
        b: [:g],
        g: [:b]
      }
    end

    describe '#run' do
      context 'with :s, :g' do
        When(:route) { dijkstra.run(:s, :g) }
        Then { route == [] }
      end
    end
  end

  context 'with a network topology graph' do
    Given(:graph) do
      {
        Pio::Mac.new('11:11:11:11:11:11') => ['1:1'],
        1 => ['1:1', '1:2'],
        '1:1' => [Pio::Mac.new('11:11:11:11:11:11'), 1],
        '1:2' => [1, '2:2'],
        2 => ['2:1', '2:2'],
        '2:1' => [2, Pio::Mac.new('22:22:22:22:22:22')],
        '2:2' => ['1:2', 2],
        Pio::Mac.new('22:22:22:22:22:22') => ['2:1']
      }
    end

    describe '#run' do
      context("with Pio::Mac.new('11:11:11:11:11:11'),"\
              " Pio::Mac.new('22:22:22:22:22:22')") do
        When(:route) do
          dijkstra.run(Pio::Mac.new('11:11:11:11:11:11'),
                       Pio::Mac.new('22:22:22:22:22:22'))
        end
        Then do
          route == [Pio::Mac.new('11:11:11:11:11:11'),
                    '1:1', 1, '1:2', '2:2', 2, '2:1',
                    Pio::Mac.new('22:22:22:22:22:22')]
        end
      end
    end
  end
end
