# EnsembleCast

Evaluate block in first argument.

This gem is experimental.
By manipulating the stack of Ruby implementation, it provides features that is not possible with normal metaprogramming.
**Do not use in production code.**

## Installation

Add this line to your application's Gemfile:

```ruby
gem 'ensemble_cast'
```

And then execute:

    $ bundle install

Or install it yourself as:

    $ gem install ensemble_cast

## Usage

You can use `Module#ensemble(method_name)` method.

When you call `ensemble(method_name)`,
block passed to `method_name` will be evaluated in the context of the first argument of the block.

example:

```ruby
require 'ensemble_cast'

class Array
  ensemble def my_filter
    filter { |e| yield(e) }
  end
end

[-2, -1, 0, 1, 2].my_filter { even? && !negative? }
# => [0, 2]
```

<!-- ## Development -->

<!-- After checking out the repo, run `bin/setup` to install dependencies. You can also run `bin/console` for an interactive prompt that will allow you to experiment. -->

<!-- To install this gem onto your local machine, run `bundle exec rake install`. To release a new version, update the version number in `version.rb`, and then run `bundle exec rake release`, which will create a git tag for the version, push git commits and the created tag, and push the `.gem` file to [rubygems.org](https://rubygems.org). -->

## Contributing

Bug reports and pull requests are welcome on GitHub at https://github.com/tetetratra/ensemble_cast. This project is intended to be a safe, welcoming space for collaboration, and contributors are expected to adhere to the [code of conduct](https://github.com/tetetratra/ensemble_cast/blob/main/CODE_OF_CONDUCT.md).

## License

The gem is available as open source under the terms of the [MIT License](https://opensource.org/licenses/MIT).

## Code of Conduct

Everyone interacting in the EnsembleCast project's codebases, issue trackers, chat rooms and mailing lists is expected to follow the [code of conduct](https://github.com/[USERNAME]/ensemble_cast/blob/main/CODE_OF_CONDUCT.md).
