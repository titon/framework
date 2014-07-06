<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper\Xhtml;

use Titon\View\Helper\Html\FormHelper as HtmlFormHelper;

/**
 * The Formhelper is used for HTML form creation. Data is passed to the associated input fields
 * if a value is present with the request ($_POST, $_GET and $_FILES) or a model.
 *
 * @package Titon\View\Helper\Xhtml
 */
class FormHelper extends HtmlFormHelper {

    /**
     * A list of all XHTML tags used within the current helper.
     *
     * @type array
     */
    protected $_tags = [
        'input'             => '<input{attr} />',
        'textarea'          => '<textarea{attr}>{body}</textarea>',
        'label'             => '<label{attr}>{body}</label>',
        'select'            => '<select{attr}>{body}</select>',
        'option'            => '<option{attr}>{body}</option>',
        'optgroup'          => '<optgroup{attr}>{body}</optgroup>',
        'button'            => '<button{attr}>{body}</button>',
        'legend'            => '<legend>{body}</legend>',
        'form_open'         => '<form{attr}>',
        'form_close'        => '</form>',
        'fieldset_open'     => '<fieldset>',
        'fieldset_close'    => '</fieldset>'
    ];

}