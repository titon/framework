<?hh
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper\Xhtml;

use Titon\Utility\Registry;
use Titon\View\Helper\Html\BreadcrumbHelper as HtmlBreadcrumbHelper;

/**
 * The BreadcrumbHelper is primarily used for adding and generating breadcrumb lists.
 *
 * @package Titon\View\Helper\Xhtml
 */
class BreadcrumbHelper extends HtmlBreadcrumbHelper {

    /**
     * Attach the XhtmlHelper.
     */
    public function initialize() {
        $this->attachObject('html', function() {
            return Registry::factory('Titon\View\Helper\Xhtml\XhtmlHelper');
        });
    }

}