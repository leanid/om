;;; package --- Summary
;;; Commentary:
;;; hello To execute next line in doomemacs
;; set pointer on it and press: SPC+m+e+d
;;; Code:
(defun hello () "hello world!")
;; how to create constant in elisp? example?
(defconst MY-CONSTANT 10 "This is my constant")
;; how to create variable in elisp? example?
(defvar my-variable 10 "This is my variable")
;; to define a user option
(defcustom my-option 10 "This is my option" :type 'integer :group 'my-group)
;; to define a buffer local variable, use
(defvar-local my-buffer-local-variable 10 "This is my buffer local variable")
;; to execute call to newlly created function
;; set pointer to last ')' and press: SPC+m+e+e
(hello)
;;; 01-hello.el ends here
