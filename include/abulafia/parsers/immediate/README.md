# The immediate parser

This is a simple single-pass recursive call parser.

## Why this exists

This should be entirely expressible as a special case of the coro parser. 
However, doing so relies heavily on compiler optimizations.