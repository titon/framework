<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Kernel\Middleware;

use Titon\Kernel\MiddlewareList;

class Next {

    protected string $method;

    protected MiddlewareList $pipeline;

    public function __construct(MiddlewareList $pipeline, string $method) {
        $this->pipeline = $pipeline->toVector();
        $this->method = $method;
    }

    public function __invoke(Input $input, Output $output) {
        $middleware = $this->pipeline[0];

        $this->pipeline->removeKey(0);

        return call_user_func(inst_meth($middleware, $this->method), $input, $output, new Next($this->pipeline, $this->method));
    }

}
