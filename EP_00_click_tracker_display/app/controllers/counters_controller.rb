class CountersController < ApplicationController
  def index
    @counters = Counter.all
  end
end
