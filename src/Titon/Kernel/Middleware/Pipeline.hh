<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel\Middleware;

use Titon\Kernel\MiddlewareList;

class Pipeline {

    protected MiddlewareList $pipeline = Vector {};

    public function through(Middleware $middleware): this {
        $this->pipeline[] = $middleware;

        return $this;
    }

    public function runAfter(Input $input, Output $ouput): void {
        $next = new Next($this->pipeline->reverse(), 'after');

        return $next($input, $output);
    }

    public function runBefore(Input $input, Output $ouput): void {
        $next = new Next($this->pipeline, 'before');

        return $next($input, $output);
    }

}
