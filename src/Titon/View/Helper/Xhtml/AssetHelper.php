<?php
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper\Xhtml;

use Titon\Utility\Registry;
use Titon\View\Helper\Html\AssetHelper as HtmlAssetHelper;

/**
 * The AssetHelper aids in the process of including external stylesheets and scripts.
 *
 * @package Titon\View\Helper\Xhtml
 */
class AssetHelper extends HtmlAssetHelper {

    /**
     * Attach the XhtmlHelper.
     */
    public function initialize() {
        $this->attachObject('html', function() {
            return Registry::factory('Titon\View\Helper\Xhtml\XhtmlHelper');
        });
    }

}