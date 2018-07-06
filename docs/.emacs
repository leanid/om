(require 'package)
(add-to-list 'package-archives '("melpa-stable" . "https://stable.melpa.org/packages/") t)
;; Added by Package.el.  This must come before configurations of
;; installed packages.  Don't delete this line.  If you don't want it,
;; just comment it out by adding a semicolon to the start of the line.
;; You may delete these explanatory comments.
(package-initialize)

(use-package flycheck
  :ensure t
  :init (global-flycheck-mode))

(require 'rtags)
(require 'cmake-ide)
(require 'clang-format)
(require 'flycheck)
;;(require 'auto-complete-clang)

(add-hook 'c++-mode-hook
          (lambda () (add-hook 'before-save-hook #'clang-format-buffer nil 'local)))

(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(auto-save-default nil)
 '(global-display-line-numbers-mode t)
 '(indent-tabs-mode nil)
 '(package-selected-packages (quote (ac-clang dash cmake-ide clang-format)))
 '(tab-width 4)
 '(tool-bar-mode nil))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 )

(setq make-backup-files nil) ; stop creating backup~ files

(cmake-ide-setup) ; prepare cmake-ide
